% This script computes the eigenvalues of the linearized 
% matrix representations of the right-hand-side functions.
% 
% The code needs to be compiled with the "-Dcompute_rhs_operators"
% flag so that it writes out files that contain the matrices
% in sparse format during a simulation.
% 
% The eigenvalues are written out to text files and can be plotted
% in MATLAB with the script PlotEvals.m (or any other utility).

clear all;

MaxFiles = 10000;
fname_FFunction_root = 'Mat_FFunction_';
fname_extn = '.dat';
nevals = input('Enter number of eigenvalues to compute: ');
opts.tol = 1e-10;

for i=1:MaxFiles
    index = sprintf('%05d',i-1);
    % compute eigenvalues for FFunction matrix, if available
    filename_FFunction = strcat(fname_FFunction_root,index,fname_extn);
    if (exist(filename_FFunction,'file'))
        fprintf('Reading FFunction matrix from %s: ',filename_FFunction);
        fid = fopen(filename_FFunction,'r');
        ndof = fscanf(fid,'%d',1);
        fprintf('ndof = %d, ',ndof);
        A = zeros(ndof,ndof);
        nnz = 0;
        while (~feof(fid))
            coord = fscanf(fid,'%d',2);
            if (max(size(coord)) > 0)
                nnz = nnz + 1;
                A(coord(1),coord(2)) = fscanf(fid,'%f',1);
            end
        end
        fprintf('nnz = %d.\n',nnz);
        fclose(fid);
        FFunction_Mat = sparse(A);
        
        str_nevals   = strcat(sprintf('%05d',nevals),'_');
        FFunction_eval_fname  = strcat('EVals_',str_nevals,filename_FFunction);

        if (~exist(FFunction_eval_fname,'file'))
            fprintf('  Computing %5d eigenvalues of FFunction matrix... ',nevals);
            tic;
            if (nevals < ndof-1)
                lambdaFFunction = eigs(FFunction_Mat,nevals,'lm',opts);
            else
                lambdaFFunction = eig(full(FFunction_Mat));
            end
            waqt = toc;
            fprintf('%f seconds.\n',waqt);
            fprintf('  Saving eigenvalues to file %s.\n',FFunction_eval_fname);
            fid = fopen(FFunction_eval_fname,'w');
            for n = 1:nevals
                fprintf(fid,'%5d %+1.16e %+1.16e\n', ...
                        n,real(lambdaFFunction(n:n)), ...
                        imag(lambdaFFunction(n:n)));
            end
            fclose(fid);
        else
            fprintf('  Skipping eigenvalues of S matrix, file %s already exists.\n', ...
                    FFunction_eval_fname);
        end
        FlagFFunction = 1;
    else
        FlagFFunction = 0;
    end
    if ((~FlagFFunction))
        break;
    end
end